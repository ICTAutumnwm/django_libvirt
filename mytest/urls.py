from django.conf.urls import patterns, include, url
from django.conf.urls.defaults import * 
from django.conf import settings
from mytest.views import hello, current_datetime, hours_ahead, mgt, title, tree, resource, app, frametest, info, info0, ajax, json, json1, jsontree
# Uncomment the next two lines to enable the admin:
# from django.contrib import admin
# admin.autodiscover()

urlpatterns = patterns('',
    (r'^hello/$', hello),
    (r'^time/$', current_datetime),
    (r'^info/$', info),
    (r'^mgt/$', mgt),
    (r'^mgt/title$', title),
    (r'^mgt/tree$', tree),
    (r'^mgt/resource$', resource),
    (r'^mgt/app$', app),
    (r'^mgt/frametest$', frametest),
    (r'^info0/$', info0),
    (r'^ajax/$', ajax),
    (r'^ajax/json/$', json),
    (r'^ajax/tree/$', jsontree),
    (r'^ajax/json1/$', json1),

    # Examples:
    # url(r'^$', 'mytest.views.home', name='home'),
    # url(r'^mytest/', include('mytest.foo.urls')),

    # Uncomment the admin/doc line below to enable admin documentation:
    # url(r'^admin/doc/', include('django.contrib.admindocs.urls')),

    # Uncomment the next line to enable the admin:
    # url(r'^admin/', include(admin.site.urls)),
    (r'^media/(?P<path>.*)$','django.views.static.serve',{'document_root':settings.STATIC_PATH}),
    (r'^show/$', 'django.views.generic.simple.direct_to_template', {'template':'a.html'}),
    #(r'^cpu/$', 'django.views.generic.simple.direct_to_template', {'template':'cpu.html'}),
)
