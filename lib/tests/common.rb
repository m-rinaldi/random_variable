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
	rescue LoadError # 'rescue' only rescues StandardError without args
		abort("Gem '#{name}' required in order to work properly, " +
			"please install the required gem and try again.\n" +
			"\tThe installation is usually done as follows:\n" +
			"\t\tgem install #{name}");
	end
end
