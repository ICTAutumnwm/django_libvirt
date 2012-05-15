from django.conf.urls import patterns, include, url
from django.conf.urls.defaults import * 
from django.conf import settings
from mytest.views import mgt, title, tree, mgtresource, noderesource, resource, app, jsonmgt, jsontree, jsonnode, createall, json, restart, json1, info, info0
# Uncomment the next two lines to enable the admin:
# from django.contrib import admin
# admin.autodiscover()

urlpatterns = patterns('',
    (r'^mgt/$', mgt),
    (r'^mgt/title$', title),
    (r'^mgt/tree$', tree),
    (r'^mgt/mgtresource$', mgtresource),
    (r'^mgt/noderesource$', noderesource),
    (r'^mgt/resource$', resource),
    (r'^mgt/app$', app),
    (r'^ajax/tree/$', jsontree),
    (r'^ajax/jsonmgt/$', jsonmgt),
    (r'^ajax/jsonnode/$', jsonnode),
    (r'^ajax/createall/$', createall),
    (r'^ajax/json/$', json),
    (r'^ajax/restart/$', restart),
    (r'^ajax/json1/$', json1),



    # Examples:
    # url(r'^$', 'mytest.views.home', name='home'),
    # url(r'^mytest/', include('mytest.foo.urls')),

    # Uncomment the admin/doc line below to enable admin documentation:
    # url(r'^admin/doc/', include('django.contrib.admindocs.urls')),

    # Uncomment the next line to enable the admin:
    # url(r'^admin/', include(admin.site.urls)),
    (r'^media/(?P<path>.*)$','django.views.static.serve',{'document_root':settings.STATIC_PATH}),
    #(r'^show/$', 'django.views.generic.simple.direct_to_template', {'template':'a.html'}),
    #(r'^cpu/$', 'django.views.generic.simple.direct_to_template', {'template':'cpu.html'}),
)
