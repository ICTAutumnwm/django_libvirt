from django.http import HttpResponse
from django.template import Template, Context
from django.template.loader import get_template
from django.shortcuts import render_to_response

from django.utils import simplejson
from django.core import serializers
import json
import datetime
import os
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

def mgt(request): 
    return render_to_response('mgt.html')
def title(request): 
    return render_to_response('title.html')
def tree(request): 
    return render_to_response('tree.html')
def mgtresource(request): 
    return render_to_response('mgtresource.html')
def noderesource(request): 
    return render_to_response('noderesource.html')
def resource(request): 
    return render_to_response('resource.html')
def app(request): 
    return render_to_response('app.html')
def json0( request ):
    print "calulate data"
    ip = request.GET.get('ip')
    number = request.GET.get('number')
    print ip
    print number 
    #cmd="./mainC list -n " + number + " -i " + ip
    cmd="./mainC list -n 1 -i 127.0.0.1" 
    output = os.popen(cmd,'r')
    ret = output.read()
    info = ret.split(',')
    maxmem = string.atof(info[0])
    mem = string.atof(info[1])
    #cpunum = string.atof(info[2])
    cpu = string.atof(info[3])
    rx = string.atof(info[4])
    tx = string.atof(info[5])
    output.close()
    #global var
    global pre_cpu
    global pre_rx
    global pre_tx
    global flag
    #cpu
    d_cpu = cpu - pre_cpu
    pre_cpu = cpu
    cpups = d_cpu/1000000 * 100
    print cpups
    #rflow_total = rx / 1024
    #rx
    d_rx = rx - pre_rx
    pre_rx = rx
    rflow = d_rx/(1 * 1024)
    rflow_total = rx / 1024
    #tx
    d_tx = tx - pre_tx
    pre_tx = tx
    tflow = d_tx/(1 * 1024)
    tflow_total = tx / 1024
    #print "rx %" %rflow
    if flag == 1:
         rflow = 0
         tflow = 0
         cpups = 0
         flag =0
    data = [maxmem, mem, cpups, rflow, tflow, rflow_total, tflow_total]
    #print result
    data = simplejson.dumps( data, cls = QuerySetEncoder )
    return HttpResponse( data )
def jsontree( request ):
    print "dtree"
    global gnum
    gnum += 1
    sgnum = "%d "%gnum
    cmd="./mainC getstate" 
    output = os.popen(cmd,'r')
    ret = output.read()
    output.close()
    m = 0
    n = 0
    data = []
    #data = [{"id":m,"pid":0,"name":"Node1","url":"ip=127.0.0.1"},{"id":2,"pid":1,"name":"VM 1.1","url":"ip=127.0.0.1&number=1"},{"id":3,"pid":1,"name":"VM 1." + snum,"url":"ip=127.0.0.1&number=2"}]
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
    #ip = request.GET.get('ip')
    #number = request.GET.get('number')
    #print ip
    #print number
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
    #number = request.GET.get('number')
    print ip
    #print number 
    #cmd="./mainC list -n " + number + " -i " + ip
    #cmd="./mainC list -n 1 -i 127.0.0.1" 
    #output = os.popen(cmd,'r')
    #ret = output.read()
    #output.close()
    data = [1]
    #print result
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
    ip = request.GET.get('ip')
    number = request.GET.get('number')
    print ip
    print number
    #cmd="./mainC list -n " + number + " -i " + ip
    #cmd="./mainC list -n 1 -i 127.0.0.1" 
    #output = os.popen(cmd,'r')
    #ret = output.read()
    #output.close()
    data = [1]
    data = simplejson.dumps( data, cls = QuerySetEncoder )
    return HttpResponse( data )

def json1( request ):
    print "calulate data"
    ip = request.GET.get('ip')
    number = request.GET.get('number')
    print ip
    print number
    data = [1000002, 33232, 13, 23, 33, 100, 200]
    data = simplejson.dumps( data, cls = QuerySetEncoder )
    return HttpResponse( data )
def jsontree0( request ):
    print "dtree"
    global num
    num += 1
    snum = "%d "%num
    print num
    cmd="./jsonlist" 
    output = os.popen(cmd,'r')
    ret = output.read()
    m = 0
    n = 0
    #data = [{"id":1,"pid":0,"name":"Node1","url":"ip=127.0.0.1"},{"id":2,"pid":1,"name":"VM 1.1","url":"ip=127.0.0.1&number=1"},{"id":3,"pid":1,"name":"VM 1." + snum,"url":"ip=127.0.0.1&number=2"}]
    data = []
    tmp = {"id":1,"pid":0,"name":"Node1","url":"ip=127.0.0.1"}
    data.append(tmp)
    tmp = {"id":2,"pid":1,"name":"VM 1.1","url":"ip=127.0.0.1&number=1"}
    data.append(tmp)
    tmp = {"id":3,"pid":1,"name":"VM 1." + snum,"url":"ip=127.0.0.1&number=2"}
    data.append(tmp)
    data = simplejson.dumps( data, cls = QuerySetEncoder )
    return HttpResponse( data )
def info(request): 
    return render_to_response('info.html')
def info0(request):
    cpu0 = mem0 = 0
    cmd="./info"
    output = os.popen(cmd,'r')
    ret=output.read()
    info=ret.split(',')
    cpu = string.atof(info[0])
    mem = string.atof(info[1])
    pre_rtraffic = pre_ttraffic = 0
    rtraffic = ttraffic = 0
    d_rtraffic = rtraffic - pre_rtraffic
    d_ttraffic = ttraffic - pre_ttraffic
    print cpu
    print mem
    output.close()
    return render_to_response('info0.html', {'cpu':cpu, 'cpu0':cpu0, 'mem':mem, 'mem0':mem0})
