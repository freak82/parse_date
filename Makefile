org:
	hyperfine -m 20 "g++ -Wall -Wextra -Werror -std=c++23 -o parse_date parse_date_org.cpp"

mod:
	hyperfine -m 20 "g++ -Wall -Wextra -Werror -std=c++23 -o parse_date parse_date_mod.cpp"

mod2:
	hyperfine -m 20 "g++ -Wall -Wextra -Werror -std=c++23 -o parse_date parse_date_mod2.cpp"

mod3:
	hyperfine -m 20 "g++ -Wall -Wextra -Werror -std=c++23 -o parse_date parse_date_mod3.cpp"

clean:
	rm parse_date

.PHONY: all clean
