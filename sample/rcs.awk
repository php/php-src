BEGIN {
    sw = 40.0;
    dw = 78.0;
    hdw = dw / 2.0;
    w = 20.0;
    h =1.0;
    d = 0.2;
    ss="abcdefghijklmnopqrstuvwxyz0123456789!#$%^&*()-=\\[];'`,./";
    rnd = srand();
}

{
    xr = -hdw; y = h * 1.0; maxxl = -999;
    s = "";
    while (xr < hdw) {
	x = xr * (1 + y) - y * w / 2;
	i = (x / (1 + h) + sw /2);
	c = (0 < i && i < length($0)) ? substr($0, i, 1) : "0";
	y = h - d * c;
	xl = xr - w * y / (1 + y);
	if (xl < -hdw || xl >= hdw || xl <= maxxl) {
	    t = rand() * length(ss);
	    c = substr(ss, t, 1);
	}
	else {
	    c = substr(s, xl + hdw, 1);
	    maxxl = xl;
	}
	s = s c;
	xr = xr + 1;
    }
    print s;
}
