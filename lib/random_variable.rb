################################################################################
#                                                                              #
# File:     random_variable.rb                                                 #
#                                                                              #
################################################################################
#                                                                              #
# Author:   Jorge F.M. Rinaldi                                                 #
# Contact:  jorge.madronal.rinaldi@gmail.com                                   #
#                                                                              #
################################################################################
#                                                                              #
# Date:     2012/10/13                                                         #
#                                                                              #
################################################################################


require_relative 'ext/random_variable'

class RandomVariable
	klass = self

	def initialize(&blk)
		@blk = blk
	end


	operators = %w(+ - * / % **)

	operators.each do |operator|
		define_method(operator) do |arg|
			if arg.is_a? klass then
				return klass.new {
					self.outcome.send(operator, arg.outcome) 
				}
			end
			klass.new { 
				self.outcome.send(operator, arg) 
			}
		end
	end

	def outcome
		@blk.call
	end

	def outcomes(times)
		ary = []
		times.times do
			ary << @blk.call
		end
		ary	
	end

end

module Math

	functions = [:acos, :acosh, :asin, :asinh, :atan, :atanh, :cbrt,
			 :cos, :cosh, :erf, :erfc, :exp, :frexp, :gamma,
			 :lgamma, :log, :log10, :log2, :sin, :sinh, :sqrt,
			 :tan, :tanh]
	klass = RandomVariable

	functions.each do |method_name|
		original_method = self.method method_name

		define_singleton_method(method_name) do |arg|
			unless arg.is_a? klass then
				return original_method.call arg
			end

			return klass.new { original_method.call(arg.outcome) }
		end
	end

end
