################################################################################
#                                                                              #
# File:     common.rb                                                          #
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

# Attemps to require the gem denoted by +name+, otherwise it displays an
# error message and exits
def required_gem(name)
	begin
		require name
	rescue LoadError # without args, 'rescue' only rescues StandardError
		abort("Gem '#{name}' required in order to work properly, " +
			"please install the required gem and try again.\n" +
			"\tThe installation is usually done as follows:\n" +
			"\t\tgem install #{name}");
	end
end

require 'test/unit'
required_gem 'shoulda'

# For holding the tests classes
module RandomVariable::Tests; end

# extend the functionality of Test::Unit::TestCase
class RandomVariable::Tests::TestCase < Test::Unit::TestCase

	private
	def self.nr_params(nr)
		# it assumes Foo::Bar::...::Baz::DistributionName for
		# the test class names
		class_obj = self.to_s.split('::').last
		class_obj = RandomVariable.const_get(class_obj)

		# wrong parameters to nr_params
		if nil == nr || !nr.is_a?(Integer) || nr < 0 then
			raise TypeError
		end

		# it takes at least one parameter
		if nr >= 1 then
			should "fail instantiating with no parameter" do
				assert_raise(ArgumentError) do
					class_obj.new
				end
			end
		
		# it takes no parameter
		elsif 0 == nr then
			should "success instantiating with no parameter" do
				begin
					class_obj.new
					assert(true)
				rescue
					assert(false)
				end
			end
		end	
	end
end
