require 'rake/testtask'

task :default => [:clean, :create] do
	puts "OK -> The gem was build"		
end

task :clean do
	sh %q%rm *.gem 2> /dev/null || echo Already cleaned%
end

task :create do
	sh %q%gem build random_variable.gemspec%
end

Rake::TestTask.new do |t|
	t.libs << "test"
	t.test_files = FileList['test/test*.rb']
	t.verbose = true
end
