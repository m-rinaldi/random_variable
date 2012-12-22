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
	should "not create a Poisson RV with no parameter" do
		assert_raise(ArgumentError) { RandomVariable::Poisson.new }	
	end

	should "not create a Poisson RV with 0 as the parameter" do
		assert_raise(ArgumentError) { RandomVariable::Poisson.new(0) }
		assert_raise(ArgumentError) { RandomVariable::Poisson.new(-0) }
		assert_raise(ArgumentError) { RandomVariable::Poisson.new(0.0) }
		assert_raise(ArgumentError) { RandomVariable::Poisson.new(-0.0)}
	end
end
