################################################################################
#                                                                              #
# File:     sampleable.rb                                                      #
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

require_relative 'samples'

module RandomVariable::Sampleable
	def outcome
		intern_outcome
	end
	alias :sample :outcome

	def outcomes(nr_samples)
		samples_ary = intern_outcomes(nr_samples)
		class << samples_ary
			include RandomVariable::Samples
		end
		samples_ary
	end
	alias :samples :outcomes
end
