from django.db import models
import random
import string

# Create your models here.
def generate_unique_code():
    length = 4
    while True:
        code = ''.join(random.choices(string.ascii_uppercase, k = length))
        if Yolo.objects.filter(code = code).count() == 0:
            break
    return code

class Yolo(models.Model):
    name = models.CharField(max_length=30)
    data = models.CharField(max_length=30)
    unit = models.CharField(max_length=30)

    def __str__(self):
        return f"{self.name} in {self.room.name}"
