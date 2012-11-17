gem_description = "Create and operate with Random Variables in Ruby\n"
distributions = %W(Bernoulli Binomial Exponential Rayleigh Continous\ Uniform Discrete\ Uniform Beta F Normal Poisson);
distributions.sort!
distributions = distributions.join "\n"
gem_description << "To date, the supported distributions are:\n" 
gem_description << distributions


Gem::Specification.new do |s|
	s.name		=	'random_variable'
	s.version	=	'0.1.0'
	s.date		=	'2012-11-17'
	s.summary	=	'Random Variables in Ruby'
	s.description	=	gem_description 
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

