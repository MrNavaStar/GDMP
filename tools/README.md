# Compiled tools
These tools that aren't present on most linux systems, but are required to build gdmp.

### [Polyfill Glibc](https://github.com/corsix/polyfill-glibc)
This tool patches the compiled binary to be compatible with older versions of glibc, allowing the project to be compiled 
on modern systems without containerization.