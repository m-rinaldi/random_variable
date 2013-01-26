################################################################################
#                                                                              #
# File:     bernoulli.rb                                                       #
#                                                                              #
################################################################################
#                                                                              #
# Author:   Jorge F.M. Rinaldi                                                 #
# Contact:  jorge.madronal.rinaldi@gmail.com                                   #
#                                                                              #
################################################################################
#                                                                              #
# Date:     2012/12/24                                                         #
#                                                                              #
################################################################################


class RandomVariable::Tests::Bernoulli < RandomVariable::Tests::TestCase
	include RandomVariable

	valid_params = [0.000001, 0.001, 0.1, 0.5, 0.9, 0.99, 0.99999]

	nr_params 1

	should "fail instantiating with a NaN parameter" do
		assert_raise(ArgumentError) { Bernoulli.new(0.0/0) }
		assert_raise(ArgumentError) { Bernoulli.new(0/0.0) }
	end

	should "fail instantiating with a Infinty parameter" do
		assert_raise(ArgumentError) { Bernoulli.new(0.0/0) }
		assert_raise(ArgumentError) { Bernoulli.new(-0.0/0) }
	
		huge_number = 2**1024
		assert_raise(ArgumentError) { Bernoulli.new(huge_number) }
	end

	should "fail instantiating with a negative parameter" do
		assert_raise(ArgumentError) { Bernoulli.new(-0.0001) }
		assert_raise(ArgumentError) { Bernoulli.new(-1) }
		assert_raise(ArgumentError) { Bernoulli.new(-113) }
		assert_raise(ArgumentError) { Bernoulli.new(-10_000) }
	end

	should "fail instantiating with a parameter greater than one" do
		assert_raise(ArgumentError) { Bernoulli.new(1.001) }
		assert_raise(ArgumentError) { Bernoulli.new(10) }
		assert_raise(ArgumentError) { Bernoulli.new(100) }
	end

	should "fail instantiating with zero as the parameter" do
		assert_raise(ArgumentError) { Bernoulli.new(0) }
		assert_raise(ArgumentError) { Bernoulli.new(-0) }
		assert_raise(ArgumentError) { Bernoulli.new(0.0) }
		assert_raise(ArgumentError) { Bernoulli.new(-0.0) }
	end

	should "fail instantiating with one as the parameter" do
		assert_raise(ArgumentError) { Bernoulli.new(1) }
		assert_raise(ArgumentError) { Bernoulli.new(1.0) }
	end

	should "instantiate with a valid parameter" do
		valid_params.each do |param|
			begin
				Bernoulli.new param
			rescue
				assert(false)
			end
		end
	end

	should "provide equal to zero or one outcomes" do
		valid_params.each do |param|
			x = Bernoulli.new param
			samples = x.outcomes 100_000
			samples.each do |sample|
				assert(sample == 0 || sample == 1)
			end
		end	
	end

	should "reproduce the same outcomes for the same seed value" do
		seed = RandomVariable::new_seed
		x = Bernoulli.new 0.113
		samples = x.outcomes 10_000
		RandomVariable::seed = seed
		new_samples = x.outcomes 10_000
		samples.length.times do |i|
			assert_equal(samples[i], new_samples[i])
		end	
	end	

end
