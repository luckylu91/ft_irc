import re

code_patt = re.compile('\s*(\d\d\d)\s+(\w+)')

f = open('numeric_in')
lines = f.readlines()
f.close()

f = open('include/numeric_out.hpp', 'w')

f.write("#pragma once\n")

in_quotes = False
in_code = False
out_code = False
new_out_code = False
quote_line = ""

for line in lines:
	if len(line.strip()) == 0:
		continue
	m = re.match(code_patt, line)
	if m:
		f.write("\n")
		in_comment = False
		just_after_code = True
		code = m.group(1)
		name = m.group(2)
		f.write(f"#define {name} \"{code}\"\n")

	else:
		in_comment = True
		if line.strip().startswith("-"):
			just_after_code = False
		if just_after_code or line.strip().startswith("-"):
			f.write("// " + line.strip() + "\n")
		else :
			f.write("//   " + line.strip() + "\n")
