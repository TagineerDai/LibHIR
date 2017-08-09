import numpy as np
import sys
AGE_MAP = {1: 0, 18: 1, 25: 2, 35: 3, 45: 4, 50: 5, 56: 6}


class User:
    def __init__(self, line):
        segs = line.split(',')
        self.id = int(segs[0])
        self.isMale = (segs[1] == 'M')
        self.age = int(segs[2])         # 1, 18, 25, 35, 45, 50, 56
        self.occupation = int(segs[3])  # 0-20
        self.zip = int(segs[4])  # ignored
        self.movie = int(segs[5])
        self.rank = int(segs[6])

    def getDIMFeature(self):  # You may change it
        feature = np.zeros(shape=[32], dtype=np.float32)
        # 21 + 7 + 1 + 1 + 1
        feature[0] = self.id
        feature[1 + self.occupation] = 1.0
        feature[22 + AGE_MAP[self.age]] = 1.0
        feature[29] = int(self.isMale)
        feature[30] = self.movie
        feature[31] = self.rank
        return feature

    def getSVMFeature(self):
        # rank 1:user 2:movie 3:occupation 4:age_group 5:sex feature_is_empty
        feature = str(self.rank)
        feature = feature + ' 1:' + str(self.id)
        feature = feature + ' 2:' + str(self.movie)
        feature = feature + ' 3:' + str(self.occupation+1)
        feature = feature + ' 4:' + str(AGE_MAP[self.age]+1)
        feature = feature + ' 5:' + str(int(self.isMale)+1)+ '\n'
        return feature

    @staticmethod
    def readFromFile(filename):
        with open(filename, 'r') as f:
            ls = f.readlines()
        svm_users = []
        for l in ls:
            u = User(l[0:len(l) - 1])
            svm_users.append(u.getSVMFeature())
        return svm_users

if __name__ == '__main__':
    h = False
    try:
        if sys.argv[1] == 'help':
            print "usage:python transformat.py <raw_file> <format_file>"
            h = True
        raw_fname = sys.argv[1]
        svm_users = User.readFromFile(raw_fname)
        svm_fname = sys.argv[2]
    except:
        if h == False:
            print "Type help to see the usage."
        exit()
    f = open(svm_fname,'w')
    f.writelines(svm_users)
    print "Format Transfer done: from list to svm format."
    exit()
