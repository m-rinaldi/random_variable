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
	class Bernoulli < Generic
		# create a new <i>Bernoulli Random Variable</i> with 
		# parameter +p+
		def self.new(p)
			intern_new(p)
		end
	end

	class Beta < Generic
		# create a new <i>Beta Random Variable</i> with parameters 
		# +alpha+ and +beta+
		def self.new(alpha, beta)
			intern_new(alpha, beta)
		end
	end

	class Binomial < Generic
		# create a new <i>Binomial Random Variable</i> with parameters 
		# +n+ and +p+
		def self.new(n, p)
			intern_new(n, p)
		end
	end

	class ContinuousUniform < Generic
		# create a new <i>Continuous Uniform Random Variable</i> with 
		# parameters +a+ and +b+
		def self.new(a, b)
			intern_new(a, b)
		end	
	end

	class DiscreteUniform < Generic
		# create a new <i>Discrete Uniform Random Variable</i> with 
		# parameters +a+ and +b+
		def self.new(a, b)
			intern_new(a, b)
		end
	end

	class Exponential < Generic
		# create a new <i>Exponential Random Variable</i> with a mean of
		# +mean+
		def self.new(mean)
			intern_new(mean)
		end
	end

	class F < Generic
		# create a new <i>F Random Variable</i> with parameters 
		# +d1+ and +d2+
		def self.new(d1, d2)
			intern_new(d1, d2)
		end
	end

	class Normal < Generic
		# create a new <i>Normal Random Variable</i> with parameters
		# +mu+ and +sigma+
		def self.new(mu = 0.0, sigma = 1.0)
			intern_new(mu, sigma)
		end
	end

	class Pareto < Generic
		# create a new <i>Pareto Random Variable</i> where +a+ is the
		# shape and +m+ is the location
		def self.new(a, m = 1.0)
			intern_new(a, m)
		end
	end

	class Poisson < Generic
		# create a new <i>Poisson Random Variable</i> with a mean of
		# +mean+
		def self.new(mean)
			intern_new(mean)
		end
	end

	class Rademacher < Generic
		# create a new <i>Rademacher Random Variable</i>
		def self.new
			intern_new
		end
	end

	class Rayleigh < Generic
		# create a new <i>Rayleigh Random Variable</i> with parameter
		# +sigma+
		def self.new(sigma)
			intern_new(sigma)
		end
	end
end
