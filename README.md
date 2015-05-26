miniz
=====

Patches on top of the original svn repo.

https://code.google.com/p/miniz/

svn checkout http://miniz.googlecode.com/svn/trunk/ miniz-read-only

This git repo also includes all the upstream SVN checkins too.

I have:
* Rearranged the file structure to make it easier to #include into my project,
* Added a CMake build (just for the examples, but could also be used to build it as a library),
* Fix various compiler warnings (GCC, and later will fix for MSVC 2010).
