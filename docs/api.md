# COFFI API

@page api API

@section api_classes Classes

See also @ref accessors.

- @ref COFFI::coffi                        "coffi":                        @copybrief COFFI::coffi

- File headers:
  - @ref COFFI::dos_header                   "dos_header":                   @copybrief COFFI::dos_header
  - @ref COFFI::coff_header                  "coff_header":                  @copybrief COFFI::coff_header
  - @ref COFFI::coff_header_impl_tmpl        "coff_header_impl_tmpl":        @copybrief COFFI::coff_header_impl_tmpl
  - @ref COFFI::coff_header_impl             "coff_header_impl":             @copybrief COFFI::coff_header_impl
  - @ref COFFI::coff_header_impl_ti          "coff_header_impl_ti":          @copybrief COFFI::coff_header_impl_ti
  - @ref COFFI::optional_header              "optional_header":              @copybrief COFFI::optional_header
  - @ref COFFI::optional_header_impl_tmpl    "optional_header_impl_tmpl":    @copybrief COFFI::optional_header_impl_tmpl
  - @ref COFFI::optional_header_impl_pe      "optional_header_impl_pe":      @copybrief COFFI::optional_header_impl_pe
  - @ref COFFI::optional_header_impl_pe_plus "optional_header_impl_pe_plus": @copybrief COFFI::optional_header_impl_pe_plus
  - @ref COFFI::optional_header_impl_ti      "optional_header_impl_ti":      @copybrief COFFI::optional_header_impl_ti
  - @ref COFFI::win_header                   "win_header":                   @copybrief COFFI::win_header
  - @ref COFFI::win_header_impl              "win_header_impl":              @copybrief COFFI::win_header_impl
  - @ref COFFI::directory                    "directory":                    @copybrief COFFI::directory
  - @ref COFFI::directories                  "directories":                  @copybrief COFFI::directories
  - Sections:
  - @ref COFFI::sections                     "sections":                     @copybrief COFFI::sections
  - @ref COFFI::section                      "section":                      @copybrief COFFI::section
  - @ref COFFI::section_impl_tmpl            "section_impl_tmpl":            @copybrief COFFI::section_impl_tmpl
  - @ref COFFI::section_impl                 "section_impl":                 @copybrief COFFI::section_impl
  - @ref COFFI::section_impl_ti              "section_impl_ti":              @copybrief COFFI::section_impl_ti
  - @ref COFFI::relocation                   "relocation":                   @copybrief COFFI::relocation
  - Symbols and strings:
  - @ref COFFI::coffi_symbols                "coffi_symbols":                @copybrief COFFI::coffi_symbols
  - @ref COFFI::symbol                       "symbol":                       @copybrief COFFI::symbol
  - @ref COFFI::coffi_strings                "coffi_strings":                @copybrief COFFI::coffi_strings

@section api_interfaces Interfaces

- Interfaces
  - @ref COFFI::architecture_provider        "architecture_provider":        @copybrief COFFI::architecture_provider
  - @ref COFFI::sections_provider            "sections_provider":            @copybrief COFFI::sections_provider
  - @ref COFFI::symbol_provider              "symbol_provider":              @copybrief COFFI::symbol_provider
  - @ref COFFI::string_to_name_provider      "string_to_name_provider":      @copybrief COFFI::string_to_name_provider

@section group_structs Structures that map directly to the COFF binary files formats

- Structures that map directly to the COFF binary files formats
  - @ref COFFI::msdos_header                 "msdos_header":                 @copybrief COFFI::msdos_header
  - COFF headers:
  - @ref COFFI::coff_file_header             "coff_file_header":             @copybrief COFFI::coff_file_header
  - @ref COFFI::coff_file_header_ti          "coff_file_header_ti":          @copybrief COFFI::coff_file_header_ti
  - COFF optional headers:
  - @ref COFFI::coff_optional_header_pe      "coff_optional_header_pe":      @copybrief COFFI::coff_optional_header_pe
  - @ref COFFI::coff_optional_header_pe_plus "coff_optional_header_pe_plus": @copybrief COFFI::coff_optional_header_pe_plus
  - @ref COFFI::common_optional_header_ti    "common_optional_header_ti":    @copybrief COFFI::common_optional_header_ti
  - Windows NT headers:
  - @ref COFFI::win_header_pe                "win_header_pe":                @copybrief COFFI::win_header_pe
  - @ref COFFI::win_header_pe_plus           "win_header_pe_plus":           @copybrief COFFI::win_header_pe_plus
  - Symbols:
  - @ref COFFI::symbol_record                "symbol_record":                @copybrief COFFI::symbol_record
  - @ref COFFI::auxiliary_symbol_record      "auxiliary_symbol_record":      @copybrief COFFI::auxiliary_symbol_record
  - @ref COFFI::auxiliary_symbol_record_1    "auxiliary_symbol_record_1":    @copybrief COFFI::auxiliary_symbol_record_1
  - @ref COFFI::auxiliary_symbol_record_2    "auxiliary_symbol_record_2":    @copybrief COFFI::auxiliary_symbol_record_2
  - @ref COFFI::auxiliary_symbol_record_3    "auxiliary_symbol_record_3":    @copybrief COFFI::auxiliary_symbol_record_3
  - @ref COFFI::auxiliary_symbol_record_4    "auxiliary_symbol_record_4":    @copybrief COFFI::auxiliary_symbol_record_4
  - @ref COFFI::auxiliary_symbol_record_5    "auxiliary_symbol_record_5":    @copybrief COFFI::auxiliary_symbol_record_5
  - @ref COFFI::image_data_directory         "image_data_directory":         @copybrief COFFI::image_data_directory
  - Section headers:
  - @ref COFFI::section_header               "section_header":               @copybrief COFFI::section_header
  - @ref COFFI::section_header_ti            "section_header_ti":            @copybrief COFFI::section_header_ti
  - Relocation entries:
  - @ref COFFI::rel_entry                    "rel_entry":                    @copybrief COFFI::rel_entry
  - @ref COFFI::rel_entry_ti                 "rel_entry_ti":                 @copybrief COFFI::rel_entry_ti
  - @ref COFFI::rel_entry_ceva               "rel_entry_ceva":               @copybrief COFFI::rel_entry_ceva
  - @ref COFFI::rel_entry_generic            "rel_entry_generic":            @copybrief COFFI::rel_entry_generic
  - @ref COFFI::line_number                  "line_number":                  @copybrief COFFI::line_number
