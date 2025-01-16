
#include "XYCurveData.h"


///////////////////////////////////////////////////////XYData////////////////////////////////////////////////////////////

void XYCurve::SetData(t_xydata_live* newData)
{
    this->xydata = newData;
}

QColor XYCurve::GetColor()
{
    return this->color;
}

int XYCurve::GetXId()
{
    return this->pX->currentIndex();
}

int XYCurve::GetYId()
{
    return this->pY->currentIndex();
}

void XYCurve::RollData()
{
    for( int i = 0; i < buffSize; i++)
    {
        this->xNorm[i] = this->x[(this->lastPosCycled + i + buffSize) % buffSize];
        this->yNorm[i] = this->y[(this->lastPosCycled + i + buffSize) % buffSize];
    }
}

void XYCurve::XYParamPickWrapper(int curX)
{
    XYParamPick(curX);
}

void XYCurve::XYParamPick(int curX, int samplesToCheck)
{
    if(this->GetXId() > 0 && this->GetYId() > 0)
    {
        this->curve->detach();
        this->StepPickedData(GetXId() - 1, GetYId() - 1, curX, samplesToCheck);
        RollData();
        this->curve->setSamples(this->xNorm, this->yNorm, buffSize);
        this->curve->setPen(this->color);
        this->curve->attach(this->plot);
        this->plot->replot();
    }
}

void XYCurve::Delete()
{
    delete this;
}

void XYCurve::SetParentDataList(QList<XYCurve*> *list)
{
    this->parentDataList = list;
}

void XYCurve::SetParentFrameList(QList<QFrame *> *list)
{
    this->parentFrameList = list;
}

double XYCurve::FindDistace(double x1, double y1, double x2, double y2)
{
    return sqrt( pow((int)(x2 - x1),2) + pow((int)(y2 - y1),2) );
}

void XYCurve::StepPickedData(int idx, int idy, int curX, int samplesToCheck)
{
    double dist = this->step->text().toDouble();
    int iNorm = 0;

    for (int i = curX - samplesToCheck; i < (curX); i++)//128
    {
        iNorm = (i + (X_SIZE)) % (X_SIZE);
        if( FindDistace(this->xydata[idx].y[iNorm],this->xydata[idy].y[iNorm],
                        this->x[(this->lastPosCycled - 1 ) % (buffSize)],
                        this->y[(this->lastPosCycled - 1 ) % (buffSize)]) > dist)
        {
            this->x[this->lastPosCycled] = this->xydata[idx].y[iNorm];
            this->y[this->lastPosCycled] = this->xydata[idy].y[iNorm];
            this->lastPosCycled = ((this->lastPosCycled + 1 + (buffSize)) % (buffSize));
        }
    }
}

///////////////////////////////////////////////////////XYCurveData////////////////////////////////////////////////////////////

void XYCurveData::AddCurve(XYCurve *curve, QFrame *dataFW)
{
    curve->SetParentDataList(&this->data);
    curve->SetParentFrameList(&this->dataWindow);
    this->data.push_back(curve);
    this->dataWindow.push_back(dataFW);
}


void XYCurveData::DeleteCurve(int id) ///а надо ли?
{
    this->data.at(id)->~XYCurve();
    this->data.removeAt(id);
    this->dataWindow.at(id)->hide();
    this->dataWindow.removeAt(id);
}

void XYCurveData::SetData(int id, double *x[X_SIZE], double *y[X_SIZE])
{
    //this->data.at(id).SetData(x,y);
}

int XYCurveData::GetXId(int id)
{
    return this->data[id]->GetXId();
}

int XYCurveData::GetYId(int id)
{
    return this->data[id]->GetYId();
}

int XYCurveData::Size()
{
    return this->data.size();
}

QColor XYCurveData::GetColor(int id)
{
    return this->data.at(id)->GetColor();
}

QFrame* XYCurveData::GetWindow(int id)
{
    return this->dataWindow.at(id);
}

XYCurve* XYCurveData::at( int id )
{
    return this->data[id];
}
