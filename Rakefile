require 'rake/testtask'

task :default => [:clean, :create] do
	puts "OK => The gem has been build"		
end

task :clean do
	sh %q%rm *.gem 2> /dev/null || echo Already cleaned%
end

task :create do
	sh %q%gem build random_variable.gemspec%
end

