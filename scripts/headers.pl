# strace -f make -j12 2>&1 | egrep -o "\w+.hpp" | sort | uniq >headers
my(@headers_used) = split("\n", `cat headers`);
my(@headers) = split("\n", `find ../include -name "*.hpp"`);

for $header (@headers)
{
	my($header_stripped) = $header;
	$header_stripped =~ s/.*\///g;

	if (grep(/^$header_stripped$/, @headers_used))
	{
		# print "$header is used\n";
	}
	else
	{
		print "$header is not used\n";
		system("git rm $header");
	}
}
