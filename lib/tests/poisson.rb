################################################################################
#                                                                              #
# File:     poisson.rb                                                         #
#                                                                              #
################################################################################
#                                                                              #
# Author:   Jorge F.M. Rinaldi                                                 #
# Contact:  jorge.madronal.rinaldi@gmail.com                                   #
#                                                                              #
################################################################################
#                                                                              #
# Date:     2012/12/22                                                         #
#                                                                              #
################################################################################

require_relative 'common.rb'

require 'test/unit'

required_gem 'shoulda'
required_gem 'random_variable'

class RandomVariable::Tests::Poisson < Test::Unit::TestCase
	include RandomVariable

	should "fail instantiating with no parameter" do
		assert_raise(ArgumentError) { Poisson.new }	
	end

	should "fail instantiating with a NaN parameter" do
		assert_raise(ArgumentError) { Poisson.new(0.0/0) }
		assert_raise(ArgumentError) { Poisson.new(0/0.0) }
	end

	should "fail instantiating with a Infinty parameter" do
		assert_raise(ArgumentError) { Poisson.new(0.0/1) }
		assert_raise(ArgumentError) { Poisson.new(-0.0/1) }
	
		huge_number = 2**1024
		assert_raise(ArgumentError) { Poisson.new(huge_number) }
	end

	should "fail instantiating with a negative parameter" do
		assert_raise(ArgumentError) { Poisson.new(-0.0001) }
		assert_raise(ArgumentError) { Poisson.new(-1) }
		assert_raise(ArgumentError) { Poisson.new(-113) }
		assert_raise(ArgumentError) { Poisson.new(-10_000) }
	end

	should "fail instantiating with zero as the parameter" do
		assert_raise(ArgumentError) { Poisson.new(0) }
		assert_raise(ArgumentError) { Poisson.new(-0) }
		assert_raise(ArgumentError) { Poisson.new(0.0) }
		assert_raise(ArgumentError) { Poisson.new(-0.0) }
	end

	should "instantiate with a valid parameter" do
		Poisson.new 0.001
		Poisson.new 0.1
		Poisson.new 1
		Poisson.new 100
		Poisson.new 10_000
	end

	should "provide greater or equal to zero outcomes" do
		[0.001, 0.1, 1, 100, 10_000, 10_000_000].each do |param|
			x = Poisson.new param
			samples = x.outcomes 100_000
			samples.each do |sample|
				assert(sample >= 0)
			end
		end	
	end

	should "reproduce the same outcomes for the same seed value" do
		seed = RandomVariable::new_seed
		x = Poisson.new 113
		samples = x.outcomes 10
		RandomVariable::seed = seed
		new_samples = x.outcomes 10
		samples.length.times do |i|
			puts "#{samples[i]} ?= #{new_samples[i]}"
			assert_equal(samples[i], new_samples[i])
		end	
	end	
	
end
