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
num = 0

def hello(request):
    return HttpResponse("Hello world")
def current_datetime(request):
    now = datetime.datetime.now()
    return render_to_response('current_datetime.html', {'current_date':now})
def ajax(request):
    return render_to_response('live-server.html')
class QuerySetEncoder( simplejson.JSONEncoder ):
    """
    Encoding QuerySet into JSON format.
    """
    def default( self, object ):
        try:
            return serializers.serialize( "python", object,ensure_ascii = False )
        except:
            return simplejson.JSONEncoder.default( self, object )
def info(request): 
    return render_to_response('info.html')
def mgt(request): 
    return render_to_response('mgt.html')
def title(request): 
    return render_to_response('title.html')
def tree(request): 
    return render_to_response('tree.html')
def resource(request): 
    return render_to_response('resource.html')
def app(request): 
    return render_to_response('app.html')
def frametest(request): 
    ip = request.GET.get('ip')
    number = request.GET.get('number')
    print ip
    print number
    return render_to_response('frametest.html')
def json( request ):
    print "calulate data"
    ip = request.GET.get('ip')
    number = request.GET.get('number')
    print ip
    print number 
    cmd="./mainC list -n " + number + " -i " + ip
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
def json1( request ):
    print "calulate data"
    ip = request.GET.get('ip')
    number = request.GET.get('number')
    print ip
    print number
    data = [1000002, 33232, 13, 23, 33, 100, 200]
    data = simplejson.dumps( data, cls = QuerySetEncoder )
    return HttpResponse( data )
def jsontree( request ):
    print "dtree"
    global num
    num += 1
    snum = "%d "%num
    print num
    m = 1
    data = {"id":m,"pid":0,"name":"Node1","url":"ip=127.0.0.1"}
    #,{"id":2,"pid":1,"name":"VM 1.1","url":"ip=127.0.0.1&number=1"}
    #,{"id":3,"pid":1,"name":"VM 1." + snum,"url":"ip=127.0.0.1&number=2"}
    data = simplejson.dumps( data, cls = QuerySetEncoder )
    return HttpResponse( data )
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
def hours_ahead(request, offset):
    try:
        offset = int(offset)
    except ValueError:
        raise Http404()
    dt = datetime.datetime.now() + datetime.timedelta(hours=offset)
	 #assert False
    html = "<html><body>In %s hour(s), it will be %s.</body></html>" % (offset, dt)
    return HttpResponse(html)
