import re

command_patt = re.compile(r'^\s*Command:\s*([A-Z]+)')
id = r'((?:[A-Z]+){3}_[A-Z]+)'
id_line_patt = re.compile(r'\s{6,}' + id + r'(?:\s+' + id + r')?')


f = open('rfc2812.txt')
lines = f.readlines()
f.close()

f = open('replies_out.txt', 'w')

for line in lines:
	if len(line.strip()) == 0:
		continue
	m = re.match(command_patt, line)
	if m:
		command = m.group(1)
		f.write(f'\n{command}:\n')
		continue
	# m = re.match(num_repl_patt, line)
	# if m:
	# 	num_list = True
	# 	continue
	# if num_list:
	m = re.match(id_line_patt, line)
	if m:
		n = len(m.groups()) - 1
		if n == 0:
			f.write('\n')
			continue
		for i in range(n):
			f.write(f'{m.group(i + 1)}\n')

		# ms = re.findall(id_patt, line)
		# print(len(ms))
		# if len(ms) == 0:
		# 	num_list = False
		# 	f.write('\n')
		# 	continue
		# for m in ms:
		# 	f.write(f'{m}\n')





