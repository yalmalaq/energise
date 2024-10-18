import plotly.express as px 
import pandas as pd 
import plotly.graph_objs as go

data = pd.read_csv('data_freq.csv')

fig = go.Figure()
fig.add_trace(go.scatter(x=data["Time (MS)"], y=data["Frequency"], mode='lines', name='Small 1'))

# bar(data,x='Time (MS)', y='Cores Active', color='Core Type',height=400, width=1200)
fig.write_image('graph_freq.png', scale=10)