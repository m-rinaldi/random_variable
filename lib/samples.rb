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
end
