@echo off
setlocal EnableDelayedExpansion
call ..\buildscripts\buildvars-setup.cmd %2
call ..\buildscripts\%1.cmd
