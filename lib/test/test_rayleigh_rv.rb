class TestRayleighRV < Test::Unit::TestCase
	def test_no_parameter
		assert_raise(ArgumentError) { RayleighRV.new }
	end

	def test_infinite_parameter
		assert_raise(ArgumentError) { RayleighRV.new(113.0 / 0) }
		x = 2**65536
		assert_raise(ArgumentError) { RayleighRV.new(x) }
	end
	
	def test_nan_parameter
		assert_raise(ArgumentError) { RayleighRV.new(0.0 / 0) }
	end

	def test_invalid_finite_paramter
		assert_raise(ArgumentError) { RayleighRV.new(-7) }
		assert_raise(ArgumentError) { RayleighRV.new(-0.0000001) }
		assert_raise(ArgumentError) { RayleighRV.new(0) }
		sigma_param = 2 * RayleighRV.sigma_max 
		assert_raise(ArgumentError) { RayleighRV.new(sigma_param) }
		
	end

	def test_max_lambda_produces_good_results
		lambda_max = RayleighRV.lambda_max
		x = RayleighRV.new(lambda_max)
		outcomes = x.outcomes(10_000_000)
		outcomes.each do |e|
			assert(e > 0)
		end
	end
end
