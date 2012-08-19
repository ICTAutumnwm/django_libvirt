from django.http import HttpResponse
from django.template import Template, Context
from django.template.loader import get_template
from django.shortcuts import render_to_response

from django.utils import simplejson
from django.core import serializers
import json
import datetime
import os
import subprocess
import string
pre_cpu = 0
pre_rx = 0
pre_tx = 0
flag = 1
gnum = 0

class QuerySetEncoder( simplejson.JSONEncoder ):
    """
    Encoding QuerySet into JSON format.
    """
    def default( self, object ):
        try:
            return serializers.serialize( "python", object,ensure_ascii = False )
        except:
            return simplejson.JSONEncoder.default( self, object )
def home(request): 
    return render_to_response('home.html')
def res(request): 
    return render_to_response('res.html')
def work(request): 
    return render_to_response('work.html')
def newvirt(request): 
    return render_to_response('newvirt.html')
def mgtresource(request): 
    return render_to_response('mgtresource.html')
def noderesource(request): 
    return render_to_response('noderesource.html')
def resource(request): 
    return render_to_response('resource.html')

def newvirt_result(request): 
    if 'fname' in request.POST:
        message = 'You searched for: %r' % request.POST['fname']
    else:
        message = 'You submitted an empty form.'
    return HttpResponse(message)
def jsontree( request ):
    print "dtree"
    #global gnum
    #gnum += 1
    #sgnum = "%d "%gnum
    #cmd="./mainC getstate" 
    cmd = "./jsonlist"
    output = os.popen(cmd,'r')
    ret = output.read()
    output.close()
    m = 0
    n = 0
    data = []
    l=len(ret)
    ndcount = ret.count('node')
    for i in range(0, ndcount):
        #get node s e
        nds = ret.find('node')
        #print nds
        ret = ret[nds+4:l]
        nde = ret.find('node')
        #print nde

        #get node
        ndstr = ret[0:nde]
        vmcount = ndstr.count('demo')
        ipend = ndstr.find('demo')
        ip = ndstr[0:ipend]
        m = m + 1
        n = m
        tmp = {"id":m,"pid":0,"name":ip,"url":"noderesource?ip="+ip}
        data.append(tmp)

        ndlen = len(ndstr)
        l=len(ret)
        ret = ret[nde:l]
        for i in range(0,vmcount):
	         #get vm s e
	         vms = ndstr.find('demo')
	         ndstr = ndstr[vms+4:ndlen]
	         vme = ndstr.find('demo')
	         if vme == -1:
		          vme = ndlen
	         #print "vme:%s" %vme
	         #get vm
	         num = ndstr[0:vme]
	         m = m + 1
	         tmp = {"id":m,"pid":n,"name":'VM'+num,"url":"resource?ip="+ip+"&number="+num}
	         data.append(tmp)
	         ndlen = len(ndstr)
	         ndstr = ndstr[vme:ndlen]

    #tmp = {"id":m+1,"pid":n,"name":"Node"+sgnum,"url":"ip=127.0.0.1"}
    #data.append(tmp)
    #print data
    #tmp = {"id":2,"pid":1,"name":"VM 1.1","url":"ip=127.0.0.1&number=1"}
    #data.append(tmp)
    #tmp = {"id":3,"pid":1,"name":"VM 1." + snum,"url":"ip=127.0.0.1&number=2"}
    #data.append(tmp)
    data = simplejson.dumps( data, cls = QuerySetEncoder )
    return HttpResponse( data )
def jsonmgt( request ):
    print "calulate data"
    cmd="./mgtinfo" 
    output = os.popen(cmd,'r')
    ret = output.read()
    info = ret.split(',')
    maxmem = string.atof(info[0])
    mem = string.atof(info[1])
    cpu_use = string.atof(info[2])
    cpu_total = string.atof(info[3])
    rb = string.atof(info[4])
    tb = string.atof(info[5])
    output.close()
    data = [maxmem,mem,cpu_use,cpu_total,rb,tb]
    data = simplejson.dumps( data, cls = QuerySetEncoder )
    return HttpResponse( data )
def jsonnode(request):
    print "node data"
    ip = request.GET.get('ip')
    print ip
    cmd="./mainC listnode -i " + ip
    output = os.popen(cmd,'r')
    ret = output.read()
    info = ret.split(',')
    maxmem = string.atof(info[0])
    mem = string.atof(info[1])
    cpu_use = string.atof(info[2])
    cpu_total = string.atof(info[3])
    rb = string.atof(info[4])
    tb = string.atof(info[5])
    output.close()
    data = [maxmem,mem,cpu_use,cpu_total,rb,tb]
    print ret
    data = simplejson.dumps( data, cls = QuerySetEncoder )
    return HttpResponse( data )
def createall( request ):
    print "createall"
    ip = request.GET.get('ip')
    #number = request.GET.get('number')
    print ip
    #print number
    #cmd="./mainC list -n " + number + " -i " + ip
    #cmd="./mainC list -n 1 -i 127.0.0.1" 
    #output = os.popen(cmd,'r')
    #ret = output.read()
    #output.close()
    data = [1]
    data = simplejson.dumps( data, cls = QuerySetEncoder )
    return HttpResponse( data )
def json(request):
    print "calulate data"
    ip = request.GET.get('ip')
    number = request.GET.get('number')
    print ip
    print number 
    cmd="./mainC list -n " + number + " -i " + ip
    #cmd="./mainC list -n 1 -i 127.0.0.1" 
    output = os.popen(cmd,'r')
    ret = output.read()
    info = ret.split(',')
    maxmem = string.atof(info[0])
    mem = string.atof(info[1])
    cpunum = string.atof(info[2])
    cpu = string.atof(info[3])
    rx = string.atof(info[4])
    tx = string.atof(info[5])
    output.close()
    data = [maxmem,mem,cpunum,cpu,rx,tx]
    #print result
    data = simplejson.dumps( data, cls = QuerySetEncoder )
    return HttpResponse( data )
def restart( request ):
    print "restarting"
    rebootip = request.GET.get('rebootip')
    #number = request.GET.get('number')
    print rebootip
    #print number
    cmd="./mainC reboot -i " + rebootip
    output = os.popen(cmd,'r')
    ret = output.read()
    output.close()
    data = [1]
    data = simplejson.dumps( data, cls = QuerySetEncoder )
    return HttpResponse( data )


