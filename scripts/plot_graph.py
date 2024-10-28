import plotly.express as px 
import pandas as pd 
import plotly.graph_objs as go

data = pd.read_csv('graph2.csv')

fig = px.bar(data,x='Time (MS)', y='Cores Active', color='Core Type',height=800, width=1200)
fig.write_image('graph2.png', scale=10)