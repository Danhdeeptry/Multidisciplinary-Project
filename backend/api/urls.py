from django.urls import path
from .views import YoloListCreate, YoloRetrieve

urlpatterns = [
    path('yolo/', YoloListCreate.as_view(), name='yolo-list-create'),
    path('yolo/<int:pk>/', YoloRetrieve.as_view(), name='yolo-retrieve'),
]