struct img_in {
	string	host<>;
	int		port;
	string	uri<>;
};

struct img_out {
	opaque buffer<>;
	int final;
};

program IMG_PROG {
	version IMG_VERS {
		img_out IMG_PROC(img_in) = 1;
	} = 1;
} = 0x22222222;
