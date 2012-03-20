while (<>) {
    for (split(/\W+/)) {
	$freq{$_}++;
    }
}

for (sort keys %freq) {
    print "$_ -- $freq{$_}\n";
}
