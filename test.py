s="ECTA RTYRPC RTCLQQP WFRRLRP"
for i in range(26):
	for ch in s:
		if ch==' ':
			print(end=" ")
			continue
		t=ord(ch)-ord('A')+26
		t+=i
		t%=26
		t+=ord('A')
		print(chr(t),end="")
		j+=1
	print()
'''
cary ginger 
'''