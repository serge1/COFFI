AM_CPPFLAGS        = -I$(top_srcdir)/..
bin_PROGRAMS       = coffi_test COFFDump_test

coffi_test_SOURCES = coffi_test.cpp
coffi_test_LDADD   = -lboost_unit_test_framework

COFFDump_test_SOURCES = COFFDump_test.cpp
COFFDump_test_LDADD   = -lboost_unit_test_framework -lboost_filesystem

TESTS=coffi_test COFFDump_test
