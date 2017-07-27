import numpy as np
import os
import random
import sys
from optparse import OptionParser
from sklearn.preprocessing import OneHotEncoder

# Data split
wfname = "svm.txt"
Rtrain = 0.8
#arti-specify for model
task = 0
hnum = 1
dim = 10
#auto-specify by data
fwid = 0
entity = 0
rnum = 0
rtrain = 0 # Rtrain*rnum
rtest = 0 # rnum - Rtrain
nylist = []
ylist = [] # the raw value of y for every record
y_classes = []
flist = [] # list of features # wid [string]
elist = [] # list of indexes # imax [string]
rlist = [] # The number or known entity index number # rnum
pairs = [] # list of dictionary
imax = [] # the max index of every entity
iblank = [] # choosing record of every entity
size = [] # the size of the trans network
index = []


# define a dictionary class with default value
class Entity(dict):
	def __missing__(self, key):
		return 0

# list print in simplist format
def listprint(li, char):
	"""
	usage: str = listprint(ylist, ' ')
	li is the list to print
	char is the split character
	"""
	ss = ''
	for i in range(len(li)-1):
		ss = ss + str(li[i]) + char
	ss = ss + str(li[len(li)-1])
	return ss

def get_config_paras():
	global wfname, Rtrain, task, hnum, dim
	try:
		opt = OptionParser()
		opt.add_option('--rawfile',
			dest = 'wfname', type = 'string', default = wfname,
			help = 'The raw file name to be preprocess')
		opt.add_option('--Rtrain',
			dest = "Rtrain", type = float, default = 0.8,
			help = "The ratio of the training data to the total learning data")
		opt.add_option('--category',
			dest = 'task', type=int, default = 1,
			help='The type of the task [1-regression|2-biClass|...-multiClass]')
		opt.add_option('--hlayer',
			dest = 'hnum', type = int, default = 1,
			help = "Number of the hidden layer")
		opt.add_option('--dim',
			dest = "dim", type = int, default = 10,
			help = "dimension of the item feature")
		(options, args) = opt.parse_args()
		is_valid_paras = True
		error = []
		if options.Rtrain>1 or options.Rtrain<0:
			error.append("The train ratio can only be in 0 and 1\n")
			is_valid_paras = False
		if options.dim<2:
			error.append("The dimension of feature should be larger than 1\n")
			is_valid_paras = False
		if is_valid_paras:
			wfname = options.wfname
			Rtrain = options.Rtrain
			task = options.task
			hnum = options.hnum
			dim = options.dim
		else:
			for msg in error:
				print msg
			opt.print_help()
			return None
	except Exception as ex:
		print("Exception :{0}".format(str(ex)))
	return None

def reshape(X_list):
	"""Covert the x-dim list into 1*x 2-dim list."""
	Y_list = []
	for i in range(len(X_list)):
		sig_list = []
		sig_list.append(X_list[i])
		Y_list.append(sig_list)
	return Y_list

def OneHot(cate_num, from_list):
	"""
	Covert the category index into one hot format.
	usage:nylist = OneHot(type, ylist)
	"""
	if cate_num < 2:
		return reshape(from_list)
	OHlist = []
	enc = OneHotEncoder()
	enc.fit(reshape(range(cate_num)))
	OHlist = enc.transform(reshape(from_list)).toarray()
	return OHlist

def outconfig(cfname):
	global rtrain, rtest, fwid, task, dim, hnum
	global entity, imax, index, size

	# finish the config.txt in format given
	config = open(cfname,'w')
	s = """###Filename
modelFile = model.txt
trainRecord = train.txt
testRecord = test.txt

### Data specification
# record number [integer]
rtrain = {0}
rtest = {1}
# feature width [vector]
fwid = {2}
# target scale [integer]
task = {3}
# model specification
model = 0
dim = {4}
hnum = {5}
enum = {6}
""".format(rtrain, rtest, fwid, task, dim, hnum, entity)
	config.write(s)
	s = """
### Size of parameter
# max entity [vector]
maxe = {0}
# entity feature index [vector]
idxe = {1}
# size of network [vector]
size = {2}""".format(listprint(imax,','),listprint(index, ','),listprint(size,','))
	config.write(s)
	config.close()

def outmodel(mfname):
	global hnum, dim, entity, task, index

	model = open(mfname, 'w')

	# entity-item features
	for i in xrange(index[len(index)-1]):
		para = []
		for j in xrange(dim):
			para.append(random.random())
		model.write(listprint(para,' ')+'\n')

	# last layer params
	# print "M ", task, " x ", dim
	for i in xrange(task):
		para = []
		for j in xrange(dim):
			para.append(random.random())
		model.write(listprint(para, ' ')+'\n')

	# hidden layer params
	# print "H ", hnum,' ', dim, ' x ', dim
	for i in xrange(hnum):
		for j in xrange(dim):
			para = []
			for k in xrange(dim):
				para.append(random.random())
			model.write(listprint(para,' ')+'\n')

	# tensor layer params
	# print "T ",entity, fwid, ' x ', dim
	for i in xrange(entity - 1):
		for i in xrange(dim):
			for j in xrange(dim):
				para = []
				for k in xrange(dim):
					para.append(random.random())
				model.write(listprint(para,' ')+'\n')
	if fwid:
		for i in xrange(dim):
			for j in xrange(fwid):
				para = []
				for k in xrange(dim):
					para.append(random.random())
				model.write(listprint(para,' ')+'\n')

	model.close()

def outrecord(ftrname, ftename):
	global rtrain, rtest, rnum, nylist, elist, fwid, flist

	train = open(ftrname,'w')
	test = open(ftename,'w')
	train.write(str(rnum) + ' ' + str(rtrain)+ '\n')
	test.write(str(rnum) + ' ' + str(rtest)+ '\n')

	#train.write(listprint(range(rnum), ' ') + '\n')

    # nylist
	for i in range(rnum):
		if (i<rtrain):
			train.write(listprint(nylist[i], ' ') + '\n')
		else:
			test.write(listprint(nylist[i], ' ') + '\n')

	# index list
	train.writelines(listprint(elist[0:rtrain], '\n'))
	test.writelines(listprint(elist[rtrain:], '\n'))
	test.write('\n')
	train.write('\n')

	# feature list
	if fwid:
		train.writelines(listprint(flist[0:rtrain], '\n'))
		test.writelines(listprint(flist[rtrain:], '\n'))
	train.close()
	test.close()

def main():
	global wfname, Rtrain
	global rnum, rtrain, rtest, fwid, task, dim, hnum, entity
	global index, size
	global ylist, nylist, flist, elist, rlist, pairs, imax, iblank

	#Parse parameter
	get_config_paras()
	fraw = open(wfname, 'r')
	#record counting
	rdata = fraw.readlines()
	rnum = len(rdata)
	rtrain = (int)(rnum * Rtrain)
	rtest = rnum - rtrain
	# in every record
	for l in rdata:
		pair = Entity()
		fstr = ''
		line = l.split()

		#y label data(before encoder)
		y = int(line[0])
		ylist.append(y)
		#e layer datals

		is_e = True
		enum = 0
		while True:
			try:
				ei = int(line[enum+1][:int(line[enum+1].find(':'))])
				ev = int(line[enum+1][int(line[enum+1].find(':'))+1:])
				pair[ei] = ev
				enum = enum + 1
				#total entity
				if cmp(entity, ei)<0:
					entity = ei
				pairs.append(pair)
			except:
				break
		r = enum
		#feature string and width
		fwid = max(fwid, len(line) - r - 1)
		fstr = ""
		if fwid:
			for fnum in range(r + 1, r + fwid - 1):
				fstr = fstr + line[fnum] + ' '
			fstr = fstr + line[r + fwid - 1]
			flist.append(fstr)

	# choosing record of every entity
	iblank = [[] for i in range(entity)]
	# format changing and max index finding of x
	for r in range(rnum):
		eplist = []
		for i in xrange(entity):
			ev = pairs[r][i+1]
			eplist.append(ev)
			iblank[i].append(ev)
		elist.append(listprint(eplist, ' '))
	tmp = 0
	for i in xrange(entity):
		imax.append(max(iblank[i]))
		index.append(tmp + imax[i])
		tmp = imax[i] + tmp

	#The size vector for model init
	for i in xrange(entity - 1):
		size.append(dim)
	if fwid:
		size.append(fwid)

	#The target value
	nylist = OneHot(task, ylist)

	outconfig("config.txt")

	outrecord("train.txt", "test.txt")

	outmodel("model.txt")

	print " PREPROCESS DONE"
	print "          /-> config.txt"
	print " svm.txt -|->train.txt/test.txt"
	print "          \-> model.txt"
if __name__ == "__main__":
	main()
