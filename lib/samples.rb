################################################################################
#                                                                              #
# File:     samples.rb                                                         #
#                                                                              #
################################################################################
#                                                                              #
# Author:   Jorge F.M. Rinaldi                                                 #
# Contact:  jorge.madronal.rinaldi@gmail.com                                   #
#                                                                              #
################################################################################
#                                                                              #
# Date:     2012/12/02                                                         #
#                                                                              #
################################################################################

module RandomVariable::Samples
	def iterate_samples
		self.each_with_index do |sample, i|
			if block_given? then
				yield(sample, i)
			end	
		end
	end
	
	def max
		cur_max = self[0]
		self.iterate_samples do |sample|
			cur_max = sample if sample > cur_max
		end
		cur_max
	end
	
	def min
		cur_min = self[0]
		self.iterate_samples do |sample|
			cur_min = sample if sample < cur_min
		end
		cur_min
	end

	def mean
		return nil if 0 == self.size
		acc = 0.0
		self.iterate_samples do |sample|
			acc += sample
		end
		acc / self.size
	end

	def median
		return nil if 0 == self.size
		return self[0] if 1 == self.size
		ary = self.sort
		i = ary.len / 2
		if ary.len.even? then
			return (ary[i] + ary[i-1]) / 2.0
		else
			return ary[i]
		end
	end
end
