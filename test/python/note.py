meteo = "soleil"
bobo = True
# {} -> dico  ie {"key":"value", "key2": ["lsitvalue1", "listvalu2"]} 
# dico peut erte acceder via dico[key] = newvalue
# [] -> lists
# () -> tuples
# tuple immuable, list modifiable
cas = []

print(f"Hello worl!, il fait {meteo}")

if bobo:
	print("ouaip")

cas.append("first")

print(cas[0])

cas.append("deusio")
cas.append("aefef")
cas.remove("first")
cas.sort()
print(cas)

print("sds" in cas)

# range(5)
for i in [0,2, 5]:
	print(i)
 

import mypckg as my
from mypckg.mymod import super

my.mymod.simple(5)
# super()

