require 'mkmf'

extension_name = 'random_variable'
have_header 'ruby.h'
have_header 'stdarg.h'
have_header 'math.h'
have_header 'limits.h'
have_header 'float.h'
create_makefile 'random_variable'
