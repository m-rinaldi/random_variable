version = '1.2.2'
date = Time.now; date = "%d-%02d-%02d" % [date.year, date.month, date.day]

Gem::Specification.new do |s|
	s.name		=	'random_variable'
	s.version	=	version
	s.date		=	date
	s.summary	=	'Random Variables in Ruby'
	s.description   = 'random variables for a wide variety ' +
		'of probability distributions such as: ' +
		'Binomial, Beta, Chi-Squared, Normal, Pareto, Poisson, etc.'

	s.authors	=	['Jorge Fco. Madronal Rinaldi']
	s.email		=	'jorge.madronal.rinaldi@gmail.com'
	s.license	=	'GPLv3'
	s.files		=	['lib/random_variable.rb']
	s.homepage	=	'http://mrinaldi.net/random_variable'	


	s.required_ruby_version = '>= 1.9.3'
	
	s.extensions << 'lib/ext/extconf.rb'

	s.files << 'lib/samples.rb'	
	s.files << 'lib/sampleable.rb'	
	s.files << 'lib/distros.rb'	
	s.files << 'LICENSE'
	s.files << 'COPYING'

	# test files
	s.files << 'lib/test.rb'
	s.files << 'lib/tests/common.rb'
	s.files << 'lib/tests/poisson.rb'

	# more files in the lib/ext directory
	s.files << 'lib/ext/extconf.rb'
	s.files << 'lib/ext/random_variable.c'
	s.files << 'lib/ext/gen.c'
	s.files << 'lib/ext/gen.h'
	s.files << 'lib/ext/randlib.c'
	s.files << 'lib/ext/randlib.h'
	s.files << 'lib/ext/xrandlib.c'
	s.files << 'lib/ext/xrandlib.h'
	s.files << 'lib/ext/linpack.c'
	s.files << 'lib/ext/com.c'

end

