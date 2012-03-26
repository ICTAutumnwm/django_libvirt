from django.http import HttpResponse
from django.template import Template, Context
from django.template.loader import get_template
from django.shortcuts import render_to_response

from django.utils import simplejson
from django.core import serializers
import datetime
import os
import string
pre_cputime = 0
pre_rx = 0
pre_tx = 0
pre_rx_total = 0
pre_tx_total = 0
cputime = 0
rx = 0
tx = 0
rx_total = 0
tx_total = 0
d_cputime = 0
d_rx = 0
d_tx = 0
d_rx_total = 0
d_tx_total = 0
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
def json( request ):
    print "calulate data"
    #result = {'id': 1, 'name': "ab"}
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
    #pre_rtraffic = pre_ttraffic = 0
    #rtraffic = ttraffic = 0
    #d_rtraffic = rtraffic - pre_rtraffic
    #d_ttraffic = ttraffic - pre_ttraffic
    #print ret
    #cpu = string.atof(info[0])
    #mem = string.atof(info[1])
    output.close()
    data = [maxmem,mem,cpu,rx,tx]
    #print result
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
def hours_ahead(request, offset):
    try:
        offset = int(offset)
    except ValueError:
        raise Http404()
    dt = datetime.datetime.now() + datetime.timedelta(hours=offset)
	 #assert False
    html = "<html><body>In %s hour(s), it will be %s.</body></html>" % (offset, dt)
    return HttpResponse(html)
