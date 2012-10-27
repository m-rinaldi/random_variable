################################################################################
#                                                                              #
# File:     test_poisson_rv.rb                                                 #
#                                                                              #
################################################################################
#                                                                              #
# Author:   Jorge F.M. Rinaldi                                                 #
# Contact:  jorge.madronal.rinaldi@gmail.com                                   #
#                                                                              #
################################################################################
#                                                                              #
# Date:     2012/10/27                                                         #
#                                                                              #
################################################################################

class TestPoissonRV < Test::Unit::TestCase
	def test_no_parameter
		assert_raise(ArgumentError) { PoissonRV.new }
	end

	def test_infinite_parameter
		assert_raise(ArgumentError) { PoissonRV.new(113.0 / 0) }
		x = 2**65536
		assert_raise(ArgumentError) { PoissonRV.new(x) }
	end
	
	def test_nan_parameter
		assert_raise(ArgumentError) { PoissonRV.new(0.0 / 0) }
	end

	def test_invalid_finite_paramter
		assert_raise(ArgumentError) { PoissonRV.new(-7) }
		assert_raise(ArgumentError) { PoissonRV.new(-0.0000001) }
		assert_raise(ArgumentError) { PoissonRV.new(0) }
		lambda_param = 2 * PoissonRV.lambda_max 
		assert_raise(ArgumentError) { PoissonRV.new(lambda_param) }
		
	end

	def test_max_lambda_produces_good_results
		lambda_max = PoissonRV.lambda_max
		x = PoissonRV.new(lambda_max)
		outcomes = x.outcomes(10_000_000)
		outcomes.each do |e|
			assert(e > 0)
		end
	end
end
