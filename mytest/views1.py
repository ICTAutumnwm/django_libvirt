from django.http import HttpResponse
from django.template import Template, Context
from django.template.loader import get_template
from django.shortcuts import render_to_response
import datetime
import os
import string
def hello(request):
    return HttpResponse("Hello world")
def current_datetime(request):
    now = datetime.datetime.now()
    return render_to_response('current_datetime.html', {'current_date':now})
def info(request):
    cpu0 = mem0 = 0
    cmd="./mainC list -n 1 -i 127.0.0.1"
    output = os.popen(cmd,'r')
    ret=output.read()
    info=ret.split(',')
    maxmem = string.atof(info[0])
    mem = string.atof(info[1])
    cpunum = string.atoi(info[2])
    cputime = string.atol(info[3])
    rx = string.atol(info[4])
    tx = string.atol(info[5])
    pre_rtraffic = pre_ttraffic = 0
    rtraffic = ttraffic = 0
    d_rtraffic = rtraffic - pre_rtraffic
    d_ttraffic = ttraffic - pre_ttraffic
    print maxmem
    print mem
    print cpunum
    print cputime
    print rx
    print tx
    cpu = cpu0 = 0.2
    mem = mem0 = 200
    output.close()
    return render_to_response('info.html', {'cpu':cpu, 'cpu0':cpu0, 'mem':mem, 'mem0':mem0})

def hours_ahead(request, offset):
    try:
        offset = int(offset)
    except ValueError:
        raise Http404()
    dt = datetime.datetime.now() + datetime.timedelta(hours=offset)
	 #assert False
    html = "<html><body>In %s hour(s), it will be %s.</body></html>" % (offset, dt)
    return HttpResponse(html)
