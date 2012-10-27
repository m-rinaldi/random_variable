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

=begin 
    random_variable gem for the creation or random variables in Ruby
    Copyright (C) 2012 Jorge Fco. Madronal Rinaldi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
=end

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


