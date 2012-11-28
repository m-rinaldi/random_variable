version = '1.0.1'
date = Time.now; date = "%d-%02d-%02d" % [date.year, date.month, date.day]

Gem::Specification.new do |s|
	s.name		=	'random_variable'
	s.version	=	version
	s.date		=	date
	s.summary	=	'Random Variables in Ruby'
	s.description	=	'Create and operate with random variables ' +
				'in Ruby'
	s.authors	=	['Jorge Fco. Madronal Rinaldi']
	s.email		=	'jorge.madronal.rinaldi@gmail.com'
	s.license	=	'GPLv3'
	s.files		=	['lib/random_variable.rb']
	s.homepage	=	'http://mrinaldi.net/random_variable'	


	s.required_ruby_version = '>= 1.9.3'
	
	s.extensions << 'lib/ext/extconf.rb'

	s.files << 'lib/distros.rb'	
	s.files << 'LICENSE'
	s.files << 'COPYING'

	# more files in the lib/ext directory
	s.files << 'lib/ext/extconf.rb'
	s.files << 'lib/ext/random_variable.c'
	s.files << 'lib/ext/randlib.c'
	s.files << 'lib/ext/randlib.h'
	s.files << 'lib/ext/xrandlib.c'
	s.files << 'lib/ext/xrandlib.h'
	s.files << 'lib/ext/linpack.c'
	s.files << 'lib/ext/com.c'

end

