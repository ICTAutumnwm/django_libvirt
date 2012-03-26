from django.http import HttpResponse
from django.template import Template, Context
from django.template.loader import get_template
from django.shortcuts import render_to_response
import datetime
import os
import string
a = "abc"
print "Hello world"
return render_to_response('live-server.html', {'ajax':a})
