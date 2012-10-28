Gem::Specification.new do |s|
	s.name		=	'random_variable'
	s.version	=	'0.0.2.pre'
	s.date		=	'2012-10-27'
	s.summary	=	'For creating Random Variables in Ruby'
	s.description	=	'Bernoulli, Normal, Poisson, Exponential' 
	s.authors	=	['Jorge Fco. Madronal Rinaldi']
	s.email		=	'jorge.madronal.rinaldi@gmail.com'
	s.license	=	'GPLv3'
	s.files		=	['lib/random_variable.rb']
	s.homepage	=	'http://mrinaldi.net/random_variable'	


	s.extensions << 'lib/ext/extconf.rb'
	
	s.files << 'LICENSE'
	s.files << 'COPYING'

	# more files in the lib directory
	s.files << 'lib/test.rb'
	
	# more files in the lib/ext directory
	s.files << 'lib/ext/extconf.rb'
	s.files << 'lib/ext/random_variable.c'
	s.files << 'lib/ext/randlib.c'
	s.files << 'lib/ext/randlib.h'
	s.files << 'lib/ext/xrandlib.c'
	s.files << 'lib/ext/xrandlib.h'
	s.files << 'lib/ext/linpack.c'
	s.files << 'lib/ext/com.c'

	# more files in the lib/test directory
	s.files << 'lib/test/test_poisson_rv.rb'
end

