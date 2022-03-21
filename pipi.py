import re

code_patt = re.compile('\s*(\d\d\d)\s+(\w+)')

f = open('numeric_in')
lines = f.readlines()
f.close()

f = open('include/numeric_out.hpp', 'w')

f.write("#pragme once\n")

in_quotes = False
in_code = False
out_code = False
new_out_code = False

for line in lines:
	if len(line.strip()) == 0:
		continue
	m = re.match(code_patt, line)
	if m:
		f.write("\n")
		if out_code:
			out_code = False
		in_code = True
		code = m.group(1)
		name = m.group(2)
		f.write(f"#define {name} \"{code}\"\n")

	elif in_code:
		if not in_quotes:
			full_line = line.strip()
			num_quotes = line.count("\"")
			if num_quotes % 2 == 0:
				f.write(f"// {full_line}\n")
				in_code = False
				new_out_code = True
			else:
				in_quotes = True
		else:
			full_line += " " + line.strip()
			num_quotes = line.count("\"")
			if num_quotes % 2 != 0:
				f.write(f"// {full_line}\n")
				in_quotes = False
				in_code = False
				new_out_code = True

	else:
		if new_out_code:
			new_out_code = False
			out_code = True
		full_line = "// " + line.strip() + "\n"
		f.write(full_line)





