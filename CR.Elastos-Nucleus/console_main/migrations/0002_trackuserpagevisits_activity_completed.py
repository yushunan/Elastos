# Generated by Django 2.2.10 on 2020-04-13 21:34

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('console_main', '0001_initial'),
    ]

    operations = [
        migrations.AddField(
            model_name='trackuserpagevisits',
            name='activity_completed',
            field=models.BooleanField(default=False),
        ),
    ]