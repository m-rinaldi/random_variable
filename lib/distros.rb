################################################################################
#                                                                              #
# File:     distros.rb                                                         #
#                                                                              #
################################################################################
#                                                                              #
# Author:   Jorge F.M. Rinaldi                                                 #
# Contact:  jorge.madronal.rinaldi@gmail.com                                   #
#                                                                              #
################################################################################
#                                                                              #
# Date:     2012/11/28                                                         #
#                                                                              #
################################################################################

module RandomVariable
	class Bernoulli
		# create a new <i>Bernoulli Random Variable</i> with 
		# parameter +p+
		def self.new(p)
			intern_new(p)
		end
	end

	class Beta
		# create a new <i>Beta Random Variable</i> with parameters 
		# +alpha+ and +beta+
		def self.new(alpha, beta)
			intern_new(alpha, beta)
		end
	end

	class Binomial
		# create a new <i>Binomial Random Variable</i> with parameters 
		# +n+ and +p+
		def self.new(n, p)
			intern_new(n, p)
		end
	end

	class ContinuousUniform
		# create a new <i>Continuous Uniform Random Variable</i> with 
		# parameters +a+ and +b+
		def self.new(a, b)
			intern_new(a, b)
		end	
	end

	class DiscreteUniform
		# create a new <i>Discrete Uniform Random Variable</i> with 
		# parameters +a+ and +b+
		def self.new(a, b)
			intern_new(a, b)
		end
	end

	class Exponential
		# create a new <i>Exponential Random Variable</i> with a mean of
		# +mean+
		def self.new(mean)
			intern_new(mean)
		end
	end

	class F
		# create a new <i>F Random Variable</i> with parameters 
		# +d1+ and +d2+
		def self.new(d1, d2)
			intern_new(d1, d2)
		end
	end

	class Normal
		# create a new <i>Normal Random Variable</i> with parameters
		# +mu+ and +sigma+
		def self.new(mu = 0.0, sigma = 1.0)
			intern_new(mu, sigma)
		end
	end

	class Poisson
		# create a new <i>Poisson Random Variable</i> with a mean of
		# +mean+
		def self.new(mean)
			intern_new(mean)
		end
	end

	class Rayleigh	
		# create a new <i>Rayleigh Random Variable</i> with parameter
		# +sigma+
		def self.new(sigma)
			intern_new(sigma)
		end
	end
end
