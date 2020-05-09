string target = Argument("target", "Default");
string configuration = Argument("configuration", "Release");
string runtime = IsRunningOnWindows() ? "win-x64" : "linux-x64";
ConvertableDirectoryPath publishDir = Directory("publish");

void CopyLibraries(DirectoryPath folder, Func<FilePath, bool> includeFile)
{
    var output = publishDir + folder;
    EnsureDirectoryExists(output);

    string libraryExt = IsRunningOnWindows() ? "lib" : "a";
    string source = "../bin/*" + configuration + "/" + folder.GetDirectoryName();
    var files =
        GetFiles(source + "/*." + libraryExt).Concat(
        GetFiles(source + "/*.dll"));
    foreach (var file in files)
    {
        if (includeFile(file.GetFilename()))
        {
            CopyFile(file, output + file.GetFilename());
        }
    }
}

void ExecScript(string name, params string[] arguments)
{
    if (IsRunningOnWindows())
    {
        FilePath file = MakeAbsolute(File(name + ".cmd"));
        StartProcess("cmd.exe", "/C \"" + file.FullPath + "\" " + string.Join(" ", arguments));
    }
    else
    {
        StartProcess("/bin/bash", name + ".sh " + string.Join(" ", arguments));
    }
}

Task("BuildCompiler")
    .IsDependentOn("RestoreTools")
    .Does(() =>
{
    DotNetCoreBuild("../src/ILCompiler/src/ILCompiler.csproj", new DotNetCoreBuildSettings
    {
        Configuration = configuration,
        OutputDirectory = "bin",
        Runtime = runtime,
    });

    try
    {
        DotNetCoreRestore("libraries.proj", new DotNetCoreRestoreSettings
        {
            PackagesDirectory = "packages",
            Runtime = runtime,
        });
    }
    catch
    {
        // We expect the restore to fail, as you're not allowed to restore a
        // DotnetPlatform type for a project, but we only using the project to
        // get the same versions as the corert repo
    }
});

Task("BuildManaged")
    .Does(() =>
{
    ExecScript("run-script", "build-managed", configuration);
});

Task("BuildNative")
    .Does(() =>
{
    ExecScript("run-script", "build-native", configuration);
});

Task("Clean")
    .Does(() =>
{
    CleanDirectory(publishDir);
});

Task("CopyCompilerLibrariesLinux")
    .WithCriteria(IsRunningOnUnix())
    .Does(() =>
{
    var toolsDir = publishDir + Directory("tools");
    EnsureDirectoryExists(toolsDir);
    CopyFiles("../bin/*" + configuration + "/tools/jitinterface.so", toolsDir);
    CopyFiles("packages/runtime.linux-x64.microsoft.dotnet.objectwriter/*/runtimes/linux-x64/native/libobjwriter.so", toolsDir);
    CopyFiles("packages/microsoft.netcore.app.runtime.linux-x64/*/runtimes/linux-x64/native/libclrjit.so", toolsDir);
    MoveFile(toolsDir + File("libclrjit.so"), toolsDir + File("libclrjitilc.so"));
});

Task("CopyCompilerLibrariesWindows")
    .WithCriteria(IsRunningOnWindows())
    .Does(() =>
{
    var toolsDir = publishDir + Directory("tools");
    EnsureDirectoryExists(toolsDir);
    CopyFiles(@"..\bin\*" + configuration + @"\tools\jitinterface.dll", toolsDir);
    CopyFiles(@"packages\runtime.win-x64.microsoft.dotnet.objectwriter\*\runtimes\win-x64\native\objwriter.dll", toolsDir);
    CopyFiles(@"packages\microsoft.netcore.app.runtime.win-x64\*\runtimes\win-x64\native\clrjit.dll", toolsDir);
    MoveFile(toolsDir + File("clrjit.dll"), toolsDir + File("clrjitilc.dll"));
});

Task("PublishBuild")
    .Does(() =>
{
    var buildDir = publishDir + Directory("build");
    EnsureDirectoryExists(buildDir);
    CopyFiles("../bin/*" + configuration + "/build/*", buildDir);

    var netStandardDir = publishDir + Directory("tools") + Directory("netstandard");
    EnsureDirectoryExists(netStandardDir);
    CopyFiles("../bin/*" + configuration + "/tools/netstandard/*.dll", netStandardDir);
});

Task("PublishCompiler")
    .IsDependentOn("CopyCompilerLibrariesLinux")
    .IsDependentOn("CopyCompilerLibrariesWindows")
    .Does(() =>
{
    var toolsDir = publishDir + Directory("tools");
    DotNetCorePublish("ilc.csproj", new DotNetCorePublishSettings
    {
        Configuration = configuration,
        OutputDirectory = toolsDir,
        Runtime = runtime,
        SelfContained = false,
    });

    DeleteFiles(toolsDir.Path + "/*.pdb");
});

Task("PublishFramework")
    .Does(() =>
{
    var toExclude = new[]
    {
        "clrcompression.dll",
    };
    CopyLibraries("framework", file => !toExclude.Contains(file.GetFilename().ToString()));
});

Task("PublishSdk")
    .Does(() =>
{
    var toExclude = new[]
    {
        "bootstrapper",
        "bootstrappercpp",
        "PortableRuntime",
        "Runtime.ServerGC",
        "System.Private.Jit.Native",
    };
    CopyLibraries("sdk", file =>
    {
        string filename = file.GetFilenameWithoutExtension().ToString();
        return !toExclude.Any(x => filename.EndsWith(x, StringComparison.OrdinalIgnoreCase));
    });
});

Task ("Publish")
    .IsDependentOn("PublishBuild")
    .IsDependentOn("PublishCompiler")
    .IsDependentOn("PublishFramework")
    .IsDependentOn("PublishSdk");

Task("RestoreTools")
    .Does(() =>
{
    ExecScript("../init-tools");
});

Task("Default")
    .IsDependentOn("Clean")
    .IsDependentOn("BuildNative")
    .IsDependentOn("BuildManaged")
    .IsDependentOn("BuildCompiler")
    .IsDependentOn("Publish");

RunTarget(target);
