# COFFI

[![Documentation](https://readthedocs.org/projects/coffi/badge)](https://coffi.readthedocs.io/en/latest)
[![License](https://img.shields.io/github/license/serge1/COFFI)](https://github.com/serge1/COFFI/blob/master/COPYING)

COFFI is a header-only C++ library intended for reading and generating files in the COFF binary format.
It is used as a standalone library - it is not dependant on any other product or project.
Adhering to ISO C++, it compiles on a wide variety of architectures and compilers.

As the COFF binary format standard is extended or violated in many implementations,
this library only supports a very small subset of the COFF implementations:

- Windows portable executable (PE) format
- Texas Instruments COFF
- CEVA, Inc COFF

The full library documentation can be found in *[COFFI - Tutorial and User Manual](https://coffi.readthedocs.io/)*.
