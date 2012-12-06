struct square_in {
	long	arg1;
};

struct square_out {
	long	real;
};

program SQUARE_PROG {
	version SQUARE_VERS {
		square_out SQUAREPROC(square_in) = 1;
	} = 1;
} = 0x22222222;

